--幻夢境
function c26920296.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,26920296)
	c:RegisterEffect(e1)
	--draw
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(26920296,0))
	e2:SetCategory(CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_DELAY)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetRange(LOCATION_FZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c26920296.drcon)
	e2:SetTarget(c26920296.drtg)
	e2:SetOperation(c26920296.drop)
	c:RegisterEffect(e2)
	--lv
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(26920296,1))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_SUMMON_SUCCESS)
	e3:SetRange(LOCATION_FZONE)
	e3:SetCondition(c26920296.lvcon)
	e3:SetTarget(c26920296.lvtg)
	e3:SetOperation(c26920296.lvop)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e4)
	--destroy
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(26920296,2))
	e5:SetCategory(CATEGORY_DESTROY)
	e5:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e5:SetRange(LOCATION_FZONE)
	e5:SetCode(EVENT_PHASE+PHASE_END)
	e5:SetCountLimit(1)
	e5:SetCondition(c26920296.descon)
	e5:SetTarget(c26920296.destg)
	e5:SetOperation(c26920296.desop)
	c:RegisterEffect(e5)
end
function c26920296.cfilter(c,type)
	return c:IsFaceup() and c:IsType(type)
end
function c26920296.drcfilter(c,tp)
	return c:IsPreviousLocation(LOCATION_HAND+LOCATION_MZONE) and c:IsType(TYPE_MONSTER) and c:IsReason(REASON_EFFECT) and c:GetPreviousControler()==tp
end
function c26920296.drcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c26920296.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,TYPE_FUSION)
		and eg:IsExists(c26920296.drcfilter,1,nil,tp)
end
function c26920296.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
end
function c26920296.drop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
function c26920296.lvcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c26920296.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,TYPE_SYNCHRO)
end
function c26920296.lvfilter(c)
	return c:IsFaceup() and c:GetLevel()>0
end
function c26920296.lvtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c26920296.lvfilter,1,nil) end
	Duel.SetTargetCard(eg)
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
end
function c26920296.lvop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local g=eg:Filter(c26920296.lvfilter,nil):Filter(Card.IsRelateToEffect,nil,e)
	local tc=g:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_LEVEL)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetValue(1)
		tc:RegisterEffect(e1)
		tc=g:GetNext()
	end
end
function c26920296.descon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c26920296.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,TYPE_XYZ)
		and Duel.GetTurnPlayer()==tp
end
function c26920296.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if g:GetCount()==0 then return end
	local mg,lv=g:GetMaxGroup(Card.GetLevel)
	if lv==0 then return end
	local dg=mg:Filter(Card.IsDestructable,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c26920296.desop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if g:GetCount()==0 then return end
	local mg,lv=g:GetMaxGroup(Card.GetLevel)
	if lv==0 then return end
	local dg=mg:Filter(Card.IsDestructable,nil)
	if dg:GetCount()>0 then
		Duel.Destroy(dg,REASON_EFFECT)
	end
end
