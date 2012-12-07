--極炎舞－「星斗」
function c6713443.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c6713443.spcost)
	e1:SetTarget(c6713443.sptg)
	e1:SetOperation(c6713443.spop)
	c:RegisterEffect(e1)
end
function c6713443.cfilter(c)
	return c:IsSetCard(0x7c) and c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsAbleToRemoveAsCost()
end
function c6713443.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c6713443.cfilter,tp,LOCATION_GRAVE,0,7,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c6713443.cfilter,tp,LOCATION_GRAVE,0,7,7,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c6713443.filter(c,e,tp)
	return c:IsSetCard(0x79) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c6713443.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 
		and Duel.IsExistingMatchingCard(c6713443.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_GRAVE)
end
function c6713443.sfilter(c)
	return c:IsSetCard(0x7c) and c:GetCode()~=6713443 and c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsSSetable()
end
function c6713443.spop(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c6713443.filter,tp,LOCATION_GRAVE,0,ft,ft,nil,e,tp)
	if g:GetCount()==0 then return end
	local ct=Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	if ct>0 then
		local ft2=Duel.GetLocationCount(tp,LOCATION_SZONE)
		if ft2>ct then ft2=ct end
		if ft2<=0 then return end
		local sg=Duel.GetMatchingGroup(c6713443.sfilter,tp,LOCATION_DECK,0,nil)
		if sg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(6713443,0)) then
			Duel.BreakEffect()
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SET)
			local setg=sg:Select(tp,1,ft2,nil)
			Duel.SSet(tp,setg)
			Duel.ConfirmCards(1-tp,setg)
		end
	end
end
