--ドリル・シンクロン
function c56286179.initial_effect(c)
	--pierce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_PIERCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c56286179.ptg)
	c:RegisterEffect(e1)
	--draw
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(56286179,0))
	e2:SetCategory(CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetCode(EVENT_BATTLE_DAMAGE)
	e2:SetCountLimit(1)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c56286179.drcon)
	e2:SetTarget(c56286179.drtg)
	e2:SetOperation(c56286179.drop)
	c:RegisterEffect(e2)
end
function c56286179.ptg(e,c)
	return c:IsRace(RACE_WARRIOR)
end
function c56286179.drcon(e,tp,eg,ep,ev,re,r,rp)
	if ep==tp then return false end
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return d and d:IsDefencePos() and a:IsControler(tp) and a:IsRace(RACE_WARRIOR)
end
function c56286179.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c56286179.drop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
