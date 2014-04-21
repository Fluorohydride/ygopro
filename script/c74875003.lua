--ラーの使徒
function c74875003.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(74875003,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_SUMMON)
	e1:SetTarget(c74875003.target)
	e1:SetOperation(c74875003.operation)
	c:RegisterEffect(e1)
	--release limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_UNRELEASABLE_NONSUM)
	e2:SetValue(1)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_UNRELEASABLE_SUM)
	e3:SetValue(c74875003.sumval)
	c:RegisterEffect(e3)
	--
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e4:SetTargetRange(1,0)
	e4:SetTarget(c74875003.splimit)
	c:RegisterEffect(e4)
end
function c74875003.filter(c,e,tp)
	return c:GetCode()==74875003 and c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN)
end
function c74875003.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c74875003.filter,tp,LOCATION_DECK+LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK+LOCATION_HAND)
end
function c74875003.operation(e,tp,eg,ep,ev,re,r,rp)
	local ct=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ct==0 then return end
	local g=Duel.GetMatchingGroup(c74875003.filter,tp,LOCATION_DECK+LOCATION_HAND,0,nil,e,tp)
	if g:GetCount()>0 then
		local t1=g:GetFirst()
		local t2=g:GetNext()
		Duel.SpecialSummonStep(t1,0,tp,tp,false,false,POS_FACEUP)
		Duel.ConfirmCards(1-tp,t1)
		if t2 and ct>1 and Duel.SelectYesNo(tp,aux.Stringid(74875003,1)) then
			Duel.SpecialSummonStep(t2,0,tp,tp,false,false,POS_FACEUP)
			Duel.ConfirmCards(1-tp,t2)
		end
		Duel.SpecialSummonComplete()
	end
end
function c74875003.sumval(c)
	return c:GetCode()==10000000 or c:GetCode()==10000010 or c:GetCode()==10000020
end

function c74875003.splimit(e,c,sump,sumtype,sumpos,targetp,se)
	return not se:GetHandler():IsCode(74875003)
end