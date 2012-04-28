--精霊の鏡
function c35563539.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c35563539.condition)
	e1:SetTarget(c35563539.target)
	e1:SetOperation(c35563539.activate)
	c:RegisterEffect(e1)
end
function c35563539.condition(e,tp,eg,ep,ev,re,r,rp)
	return re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsActiveType(TYPE_SPELL) and re:IsHasProperty(EFFECT_FLAG_PLAYER_TARGET)
end
function c35563539.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local te=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_EFFECT)
		local ftg=te:GetTarget()
		return ftg==nil or ftg(e,tp,eg,ep,ev,re,r,rp,chk)
	end
end
function c35563539.activate(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(ev,CHAININFO_TARGET_PLAYER)
	Duel.ChangeTargetPlayer(ev,1-p)
end
