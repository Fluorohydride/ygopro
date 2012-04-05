--融合呪印生物－闇
function c52101615.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetDescription(aux.Stringid(52101615,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetLabel(0)
	e1:SetCost(c52101615.cost)
	e1:SetTarget(c52101615.target)
	e1:SetOperation(c52101615.operation)
	c:RegisterEffect(e1)
	--fusion substitute
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_FUSION_SUBSTITUTE)
	e2:SetCondition(c52101615.subcon)
	c:RegisterEffect(e2)
end
function c52101615.subcon(e)
	return e:GetHandler():IsLocation(0x1e)
end
function c52101615.filter(c,e,tp,m,gc)
	return c:IsType(TYPE_FUSION) and c:IsAttribute(ATTRIBUTE_DARK)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false) and c:CheckFusionMaterial(m,gc)
end
function c52101615.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c52101615.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then
		if e:GetLabel()~=1 then return false end
		e:SetLabel(0)
		local mg=Duel.GetMatchingGroup(Card.IsCanBeFusionMaterial,tp,LOCATION_MZONE,0,nil)
		return Duel.IsExistingMatchingCard(c52101615.filter,tp,LOCATION_EXTRA,0,1,nil,e,tp,mg,c)
	end
	local mg=Duel.GetMatchingGroup(Card.IsCanBeFusionMaterial,tp,LOCATION_MZONE,0,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c52101615.filter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp,mg,c)
	local mat=Duel.SelectFusionMaterial(tp,g:GetFirst(),mg,c)
	Duel.Release(mat,REASON_COST)
	e:SetLabel(g:GetFirst():GetCode())
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c52101615.filter2(c,e,tp,code)
	return c:IsCode(code) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c52101615.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local code=e:GetLabel()
	local tc=Duel.GetFirstMatchingCard(c52101615.filter2,tp,LOCATION_EXTRA,0,nil,e,tp,code)
	if tc then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
