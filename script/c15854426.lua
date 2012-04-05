--霞の谷の神風
function c15854426.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCode(EVENT_LEAVE_FIELD_P)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c15854426.check)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(15854426,0))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)	
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_DELAY)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_TO_HAND)
	e3:SetCountLimit(1)
	e3:SetCondition(c15854426.condition)
	e3:SetTarget(c15854426.target)
	e3:SetOperation(c15854426.operation)
	c:RegisterEffect(e3)
end
function c15854426.check(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		if tc:IsControler(tp) and tc:IsFaceup() and tc:IsAttribute(ATTRIBUTE_WIND) and tc:GetDestination()==LOCATION_HAND then
			tc:RegisterFlagEffect(15854426,RESET_EVENT+0x15e0000,0,1)
		end
		tc=eg:GetNext()
	end
end
function c15854426.cfilter(c)
	return c:GetFlagEffect(15854426)~=0
end
function c15854426.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c15854426.cfilter,1,nil)
end
function c15854426.filter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsAttribute(ATTRIBUTE_WIND)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c15854426.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and not e:GetHandler():IsStatus(STATUS_CHAINING)
		and Duel.IsExistingMatchingCard(c15854426.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c15854426.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c15854426.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
