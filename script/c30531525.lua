--魔の試着部屋
function c30531525.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c30531525.cost)
	e1:SetTarget(c30531525.target)
	e1:SetOperation(c30531525.activate)
	c:RegisterEffect(e1)
end
function c30531525.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,800) end
	Duel.PayLPCost(tp,800)
end
function c30531525.filter(c,e,tp)
	return c:IsType(TYPE_NORMAL) and c:IsLevelBelow(3) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c30531525.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c30531525.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
end
function c30531525.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.ConfirmDecktop(tp,4)
	local g=Duel.GetDecktopGroup(tp,4):Filter(c30531525.filter,nil,e,tp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if g:GetCount()>0 then
		if ft<=0 then
			Duel.SendtoGrave(g,REASON_EFFECT)
		elseif ft>=g:GetCount() then
			Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
		else
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			local sg=g:Select(tp,ft,ft,nil)
			Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
			g:Sub(sg)
			Duel.SendtoGrave(g,REASON_EFFECT)
		end
	end
	Duel.ShuffleDeck(tp)
end
