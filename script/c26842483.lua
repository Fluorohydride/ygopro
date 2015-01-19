--ジャスティス・ブリンガー
function c26842483.initial_effect(c)
	--negate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(26842483,0))
	e1:SetCategory(CATEGORY_NEGATE)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c26842483.condition)
	e1:SetTarget(c26842483.target)
	e1:SetOperation(c26842483.operation)
	c:RegisterEffect(e1)
end
function c26842483.condition(e,tp,eg,ep,ev,re,r,rp,chk)
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	return ep~=tp and loc==LOCATION_MZONE and re:IsActiveType(TYPE_MONSTER) and Duel.IsChainNegatable(ev)
		and bit.band(re:GetHandler():GetSummonType(),SUMMON_TYPE_SPECIAL)~=0
end
function c26842483.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
end
function c26842483.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
end
