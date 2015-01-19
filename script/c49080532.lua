--インフェルニティ・ビートル
function c49080532.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(49080532,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c49080532.condition)
	e1:SetCost(c49080532.cost)
	e1:SetTarget(c49080532.target)
	e1:SetOperation(c49080532.operation)
	c:RegisterEffect(e1)
end
function c49080532.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)==0
end
function c49080532.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c49080532.filter(c,e,tp)
	return c:GetCode()==49080532 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c49080532.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c49080532.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c49080532.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)>0 then return end
	local ct=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ct==0 then return end
	local g=Duel.GetMatchingGroup(c49080532.filter,tp,LOCATION_DECK,0,nil,e,tp)
	if g:GetCount()>0 then
		local t1=g:GetFirst()
		local t2=g:GetNext()
		Duel.SpecialSummonStep(t1,0,tp,tp,false,false,POS_FACEUP)
		if t2 and ct>1 and Duel.SelectYesNo(tp,aux.Stringid(49080532,1)) then
			Duel.SpecialSummonStep(t2,0,tp,tp,false,false,POS_FACEUP)
		end
		Duel.SpecialSummonComplete()
	end
end
