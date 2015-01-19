--ダーク・クリエイター
function c92719314.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c92719314.spcon)
	c:RegisterEffect(e1)
	--SpecialSummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(92719314,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetCountLimit(1)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCost(c92719314.cost)
	e2:SetTarget(c92719314.target)
	e2:SetOperation(c92719314.operation)
	c:RegisterEffect(e2)
end
function c92719314.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.GetFieldGroupCount(c:GetControler(),LOCATION_MZONE,0)==0
		and Duel.IsExistingMatchingCard(Card.IsAttribute,c:GetControler(),LOCATION_GRAVE,0,5,nil,ATTRIBUTE_DARK)
end
function c92719314.costfilter(c,e,tp)
	return c:IsAttribute(ATTRIBUTE_DARK) and c:IsAbleToRemoveAsCost()
		and Duel.IsExistingTarget(c92719314.tgfilter,tp,LOCATION_GRAVE,0,1,c,e,tp)
end
function c92719314.tgfilter(c,e,tp)
	return c:IsAttribute(ATTRIBUTE_DARK) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c92719314.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c92719314.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c92719314.tgfilter(chkc,e,tp) end
	if chk==0 then
		if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return false end
		if e:GetLabel()==1 then
			e:SetLabel(0)
			return Duel.IsExistingMatchingCard(c92719314.costfilter,tp,LOCATION_GRAVE,0,1,nil,e,tp)
		else
			return Duel.IsExistingTarget(c92719314.tgfilter,tp,LOCATION_GRAVE,0,1,nil,e,tp)
		end
	end
	if e:GetLabel()==1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local cg=Duel.SelectMatchingCard(tp,c92719314.costfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
		Duel.Remove(cg,POS_FACEUP,REASON_COST)
		e:SetLabel(0)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c92719314.tgfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c92719314.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
