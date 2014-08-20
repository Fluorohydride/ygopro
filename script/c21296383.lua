--ユニバード
function c21296383.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(21296383,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c21296383.target)
	e1:SetOperation(c21296383.operation)
	c:RegisterEffect(e1)
end
function c21296383.spfilter(c,e,tp)
	return c:IsType(TYPE_SYNCHRO) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		and c:IsCanBeEffectTarget(e)
end
function c21296383.cfilter(c,lv)
	return c:IsFaceup() and c:IsAbleToRemoveAsCost() and c:GetOriginalLevel()>=lv
end
function c21296383.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local sg=Duel.GetMatchingGroup(c21296383.spfilter,tp,LOCATION_GRAVE,0,nil,e,tp)
	if chkc then return sg:IsContains(chkc) and chkc:IsLevelBelow(e:GetLabel()) end
	if sg:GetCount()==0 then return false end
	local mg,mlv=sg:GetMinGroup(Card.GetLevel)
	local elv=e:GetHandler():GetOriginalLevel()
	local lv=(elv>=mlv) and 1 or (mlv-elv)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost()
		and Duel.IsExistingMatchingCard(c21296383.cfilter,tp,LOCATION_MZONE,0,1,e:GetHandler(),lv) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c21296383.cfilter,tp,LOCATION_MZONE,0,1,1,e:GetHandler(),lv)
	local slv=elv+g:GetFirst():GetLevel()
	g:AddCard(e:GetHandler())
	Duel.Remove(g,POS_FACEUP,REASON_COST)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	e:SetLabel(slv)
	local g=sg:FilterSelect(tp,Card.IsLevelBelow,1,1,nil,slv)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c21296383.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
