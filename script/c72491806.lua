--U.A. Midfielder
function c72491806.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCountLimit(1,72491806)
	e1:SetCondition(c72491806.spcon)
	e1:SetOperation(c72491806.spop)
	c:RegisterEffect(e1)
	--to hand and spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(72491806,0))
	e2:SetCategory(CATEGORY_TOHAND+CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1,72491807)
	e2:SetTarget(c72491806.tstg)
	e2:SetOperation(c72491806.tsop)
	c:RegisterEffect(e2)
end
function c72491806.spfilter(c)
	return c:IsFaceup() and c:IsSetCard(0xb2) and not c:IsCode(72491806) and c:IsAbleToHandAsCost()
end
function c72491806.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c72491806.spfilter,c:GetControler(),LOCATION_MZONE,0,1,nil)
end
function c72491806.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectMatchingCard(tp,c72491806.spfilter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SendtoHand(g,nil,REASON_COST)
end
function c72491806.thfilter(c,e,tp)
	return c:IsFaceup() and c:IsSetCard(0xb2) and c:IsAbleToHand()
		and Duel.IsExistingMatchingCard(c72491806.spfilter2,tp,LOCATION_HAND,0,1,nil,e,tp,c:GetCode())
end
function c72491806.spfilter2(c,e,tp,code)
	return c:IsSetCard(0xb2) and not c:IsCode(code) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c72491806.tstg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c72491806.thfilter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingTarget(c72491806.thfilter,tp,LOCATION_MZONE,0,1,e:GetHandler(),e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c72491806.thfilter,tp,LOCATION_MZONE,0,1,1,e:GetHandler(),e,tp)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c72491806.tsop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and Duel.SendtoHand(tc,nil,REASON_EFFECT)~=0 then
		if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=Duel.SelectMatchingCard(tp,c72491806.spfilter2,tp,LOCATION_HAND,0,1,1,nil,e,tp,tc:GetCode())
		if sg:GetCount()>0 then
			Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
