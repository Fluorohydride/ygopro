--バグマンX
function c87526784.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(87526784,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_SINGLE)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCondition(c87526784.condition)
	e1:SetTarget(c87526784.target)
	e1:SetOperation(c87526784.operation)
	c:RegisterEffect(e1)
end
function c87526784.cfilter(c)
	return c:IsFaceup() and c:IsCode(23915499)
end
function c87526784.spfilter(c,e,tp)
	return c:IsCode(50319138) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c87526784.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c87526784.cfilter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c87526784.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c87526784.spfilter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c87526784.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	if not Duel.IsExistingMatchingCard(c87526784.cfilter,tp,LOCATION_ONFIELD,0,1,nil) then return end
	local tc=Duel.GetFirstMatchingCard(c87526784.spfilter,tp,LOCATION_DECK,0,nil,e,tp)
	if tc then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
