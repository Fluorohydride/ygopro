--ティマイオスの眼
function c1784686.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,1784686+EFFECT_COUNT_CODE_OATH)
	e1:SetTarget(c1784686.target)
	e1:SetOperation(c1784686.activate)
	c:RegisterEffect(e1)
	--add code
	--local e2=Effect.CreateEffect(c)
	--e2:SetType(EFFECT_TYPE_SINGLE)
	--e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	--e2:SetCode(EFFECT_ADD_CODE)
	--e2:SetValue(88888888)
	--c:RegisterEffect(e2)
end
function c1784686.tgfilter(c,e,tp)
	return c:IsFaceup() and c:IsSetCard(0xa2)  
	and c:IsCanBeFusionMaterial() and not c:IsImmuneToEffect(e)
	and Duel.IsExistingMatchingCard(c1784686.spfilter,tp,LOCATION_EXTRA,0,1,nil,e,tp,c:GetCode())
end
function c1784686.spfilter(c,e,tp,code)
	local t=c.material
	return c:IsType(TYPE_FUSION)
		and c:IsCanBeSpecialSummoned(e,SUMMON_TYPE_FUSION,tp,false,false) and
		aux.Contains(t,code)
end
function c1784686.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc==0 then return c1784686.tgfilter(chkc) end
	if chk==0 then return 
		Duel.IsExistingTarget(c1784686.tgfilter,tp,LOCATION_MZONE,0,1,nil,e,tp) 
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
	end
	Duel.SelectTarget(tp,c1784686.tgfilter,tp,LOCATION_MZONE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c1784686.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<0 then return end
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=Duel.SelectMatchingCard(tp,c1784686.spfilter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp,tc:GetCode())
		local sc=sg:GetFirst()
		if sg then
			sc:SetMaterial(Group.FromCards(tc))
			Duel.SendtoGrave(tc,REASON_EFFECT+REASON_MATERIAL+REASON_FUSION)
			Duel.BreakEffect()
			Duel.SpecialSummon(sc,SUMMON_TYPE_FUSION,tp,tp,false,false,POS_FACEUP)
			sc:CompleteProcedure()
		end
	end
end