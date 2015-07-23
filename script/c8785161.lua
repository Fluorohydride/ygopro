--RR－ワイルド・ヴァルチャー
function c8785161.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c8785161.spcon)
	e1:SetCost(c8785161.spcost)
	e1:SetTarget(c8785161.sptg)
	e1:SetOperation(c8785161.spop)
	c:RegisterEffect(e1)
	if not c8785161.global_check then
		c8785161.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SUMMON_SUCCESS)
		ge1:SetLabel(8785161)
		ge1:SetOperation(aux.sumreg)
		Duel.RegisterEffect(ge1,0)
		local ge2=ge1:Clone()
		ge2:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge2:SetLabel(8785161)
		Duel.RegisterEffect(ge2,0)
	end
end
function c8785161.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(8785161)>0
end
function c8785161.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c8785161.spfilter1(c,e,tp)
	local lv=c:GetLevel()
	return lv>0 and lv<6 and c:IsSetCard(0xba) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		and Duel.IsExistingMatchingCard(c8785161.spfilter2,tp,LOCATION_HAND+LOCATION_GRAVE,0,1,c,e,tp,6-lv)
end
function c8785161.spfilter2(c,e,tp,lv)
	return c:IsSetCard(0xba) and c:GetLevel()==lv and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c8785161.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c8785161.spfilter1,tp,LOCATION_HAND+LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND+LOCATION_GRAVE)
end
function c8785161.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<2 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g1=Duel.SelectMatchingCard(tp,c8785161.spfilter1,tp,LOCATION_HAND+LOCATION_GRAVE,0,1,1,nil,e,tp)
	local tc1=g1:GetFirst()
	if not tc1 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g2=Duel.SelectMatchingCard(tp,c8785161.spfilter2,tp,LOCATION_HAND+LOCATION_GRAVE,0,1,1,tc1,e,tp,6-tc1:GetLevel())
	g1:Merge(g2)
	Duel.SpecialSummon(g1,0,tp,tp,false,false,POS_FACEUP)
end
