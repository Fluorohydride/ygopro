--究極・背水の陣
function c32603633.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c32603633.cost)
	e1:SetTarget(c32603633.tg)
	e1:SetOperation(c32603633.op)
	c:RegisterEffect(e1)
end
function c32603633.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLP(tp)>100 end
	Duel.PayLPCost(tp,Duel.GetLP(tp)-100)
end
function c32603633.filter(c,e,tp)
	return c:IsSetCard(0x3d) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		and not Duel.IsExistingMatchingCard(Card.IsCode,tp,LOCATION_MZONE,0,1,nil,c:GetCode())
end
function c32603633.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 
		and Duel.IsExistingMatchingCard(c32603633.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp)
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_GRAVE)
end
function c32603633.op(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	local g=Duel.GetMatchingGroup(c32603633.filter,tp,LOCATION_GRAVE,0,nil,e,tp)
	while g:GetCount()>0 and ft>0 do
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=g:Select(tp,1,1,nil)
		Duel.SpecialSummonStep(sg:GetFirst(), 0, tp, tp, false, false, POS_FACEUP)
		ft=ft-1
		g:Remove(Card.IsCode,nil,sg:GetFirst():GetCode())
	end
	Duel.SpecialSummonComplete()
end
