--ヒュプノシスター
function c22200403.initial_effect(c)
	--atk/def
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(800)
	e1:SetCondition(c22200403.effcon)
	e1:SetLabel(1)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e2)
	--destroy
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(22200403,0))
	e3:SetCategory(CATEGORY_DESTROY)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_BATTLE_START)
	e3:SetCondition(c22200403.effcon)
	e3:SetTarget(c22200403.destg)
	e3:SetOperation(c22200403.desop)
	e3:SetLabel(2)
	c:RegisterEffect(e3)
	--must attack
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetCode(EFFECT_MUST_ATTACK)
	e4:SetRange(LOCATION_MZONE)
	e4:SetTargetRange(0,LOCATION_MZONE)
	e4:SetCondition(c22200403.effcon)
	e4:SetLabel(3)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_MUST_BE_ATTACKED)
	e5:SetCondition(c22200403.effcon)
	e5:SetLabel(3)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_FIELD)
	e6:SetCode(EFFECT_CANNOT_EP)
	e6:SetRange(LOCATION_MZONE)
	e6:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e6:SetTargetRange(0,1)
	e6:SetCondition(c22200403.atcon)
	e6:SetLabel(3)
	c:RegisterEffect(e6)
	--draw
	local e7=Effect.CreateEffect(c)
	e7:SetDescription(aux.Stringid(22200403,1))
	e7:SetCategory(CATEGORY_DRAW)
	e7:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e7:SetCode(EVENT_BATTLE_DESTROYING)
	e7:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e7:SetCondition(c22200403.drcon1)
	e7:SetTarget(c22200403.drtg)
	e7:SetOperation(c22200403.drop)
	e7:SetLabel(4)
	c:RegisterEffect(e7)
	local e8=Effect.CreateEffect(c)
	e8:SetDescription(aux.Stringid(22200403,1))
	e8:SetCategory(CATEGORY_DRAW)
	e8:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e8:SetCode(EVENT_DESTROYED)
	e8:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e8:SetRange(LOCATION_MZONE)
	e8:SetCondition(c22200403.drcon2)
	e8:SetTarget(c22200403.drtg)
	e8:SetOperation(c22200403.drop)
	e8:SetLabel(4)
	c:RegisterEffect(e8)
end
function c22200403.effcon(e)
	local ct=0
	if Duel.GetFieldCard(0,LOCATION_SZONE,6) then ct=ct+1 end
	if Duel.GetFieldCard(0,LOCATION_SZONE,7) then ct=ct+1 end
	if Duel.GetFieldCard(1,LOCATION_SZONE,6) then ct=ct+1 end
	if Duel.GetFieldCard(1,LOCATION_SZONE,7) then ct=ct+1 end
	return ct>=e:GetLabel()
end
function c22200403.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=e:GetHandler():GetBattleTarget()
	if chk==0 then return tc and tc:IsFaceup() and tc:GetSummonType()==SUMMON_TYPE_PENDULUM end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,tc,1,0,0)
end
function c22200403.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetBattleTarget()
	if tc:IsRelateToBattle() then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c22200403.atcon(e)
	return c22200403.effcon(e)
		and Duel.IsExistingMatchingCard(Card.IsAttackable,e:GetHandlerPlayer(),0,LOCATION_MZONE,1,nil)
end
function c22200403.drcon1(e,tp,eg,ep,ev,re,r,rp)
	return c22200403.effcon(e)
		and e:GetHandler():IsRelateToBattle()
end
function c22200403.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c22200403.drop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
function c22200403.drcon2(e,tp,eg,ep,ev,re,r,rp)
	return c22200403.effcon(e)
		and bit.band(r,REASON_EFFECT)~=0 and re:GetHandler()==e:GetHandler()
end
