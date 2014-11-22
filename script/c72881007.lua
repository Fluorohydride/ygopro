--宝玉の氾濫
function c72881007.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOGRAVE+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c72881007.cost)
	e1:SetTarget(c72881007.target)
	e1:SetOperation(c72881007.activate)
	c:RegisterEffect(e1)
end
function c72881007.costfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x1034) and c:IsAbleToGraveAsCost()
end
function c72881007.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	if chk==0 then return Duel.IsExistingMatchingCard(c72881007.costfilter,tp,LOCATION_SZONE,0,4,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c72881007.costfilter,tp,LOCATION_SZONE,0,4,4,nil)
	Duel.SendtoGrave(g,REASON_COST)
end
function c72881007.filter(c,e,tp)
	return c:IsSetCard(0x1034) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c72881007.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=1 then
			return Duel.IsExistingMatchingCard(c72881007.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp)
		else
			e:SetLabel(0)
			return Duel.IsExistingMatchingCard(c72881007.filter,tp,LOCATION_SZONE,0,1,nil,e,tp)
		end
	end
	e:SetLabel(0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_GRAVE)
end
function c72881007.ctfilter(c,tp)
	return c:GetPreviousControler()==tp and c:IsLocation(LOCATION_GRAVE)
end
function c72881007.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(tp,LOCATION_ONFIELD,LOCATION_ONFIELD)
	Duel.SendtoGrave(g,REASON_EFFECT)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	g=Duel.GetOperatedGroup()
	local ct=g:FilterCount(c72881007.ctfilter,nil,1-tp)
	if ft>ct then ft=ct end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	g=Duel.SelectMatchingCard(tp,c72881007.filter,tp,LOCATION_GRAVE,0,ft,ft,nil,e,tp)
	if g:GetCount()>0 then
		Duel.BreakEffect()
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
