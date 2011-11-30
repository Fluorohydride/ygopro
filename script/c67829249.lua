--古代の採掘機
function c67829249.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c67829249.condition)
	e1:SetCost(c67829249.cost)
	e1:SetTarget(c67829249.target)
	e1:SetOperation(c67829249.operation)
	c:RegisterEffect(e1)
end
function c67829249.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x7)
end
function c67829249.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c67829249.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c67829249.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDiscardable,tp,LOCATION_HAND,0,1,e:GetHandler()) end
	Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_COST+REASON_DISCARD)
end
function c67829249.filter(c)
	return c:IsType(TYPE_SPELL) and c:IsSSetable()
end
function c67829249.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c67829249.filter,tp,LOCATION_DECK,0,1,nil) end
end
function c67829249.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOFIELD)
	local g=Duel.SelectMatchingCard(tp,c67829249.filter,tp,LOCATION_DECK,0,1,1,nil)
	local tc=g:GetFirst()
	if tc then
		Duel.SSet(tp,tc)
		Duel.ConfirmCards(1-tp,tc)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CANNOT_TRIGGER)
		e1:SetReset(RESET_EVENT+0x17a0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_CANNOT_USE_AS_COST)
		e2:SetReset(RESET_EVENT+0x17a0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e2)
	end
end
