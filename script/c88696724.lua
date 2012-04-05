--融合呪印生物－地
function c88696724.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetDescription(aux.Stringid(88696724,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetLabel(0)
	e1:SetCost(c88696724.cost)
	e1:SetTarget(c88696724.target)
	e1:SetOperation(c88696724.operation)
	c:RegisterEffect(e1)
	--fusion substitute
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_FUSION_SUBSTITUTE)
	e2:SetCondition(c88696724.subcon)
	c:RegisterEffect(e2)
end
function c88696724.subcon(e)
	return e:GetHandler():IsLocation(0x1e)
end
function c88696724.filter(c,e,tp,m,gc)
	return c:IsType(TYPE_FUSION) and c:IsAttribute(ATTRIBUTE_EARTH)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false) and c:CheckFusionMaterial(m,gc)
end
function c88696724.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c88696724.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then
		if e:GetLabel()~=1 then return false end
		e:SetLabel(0)
		local mg=Duel.GetMatchingGroup(Card.IsCanBeFusionMaterial,tp,LOCATION_MZONE,0,nil)
		return Duel.IsExistingMatchingCard(c88696724.filter,tp,LOCATION_EXTRA,0,1,nil,e,tp,mg,c)
	end
	local mg=Duel.GetMatchingGroup(Card.IsCanBeFusionMaterial,tp,LOCATION_MZONE,0,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c88696724.filter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp,mg,c)
	local mat=Duel.SelectFusionMaterial(tp,g:GetFirst(),mg,c)
	Duel.Release(mat,REASON_COST)
	e:SetLabel(g:GetFirst():GetCode())
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c88696724.filter2(c,e,tp,code)
	return c:IsCode(code) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c88696724.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local code=e:GetLabel()
	local tc=Duel.GetFirstMatchingCard(c88696724.filter2,tp,LOCATION_EXTRA,0,nil,e,tp,code)
	if tc then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
