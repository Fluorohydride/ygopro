--No.7 ラッキー・ストライプ
function c82308875.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,7,3)
	c:EnableReviveLimit()
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(82308875,0))
	e1:SetCategory(CATEGORY_ATKCHANGE+CATEGORY_DRAW+CATEGORY_SPECIAL_SUMMON+CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c82308875.cost)
	e1:SetTarget(c82308875.target)
	e1:SetOperation(c82308875.operation)
	c:RegisterEffect(e1)
end
c82308875.xyz_number=7
function c82308875.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c82308875.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DICE,nil,0,tp,2)
end
function c82308875.spfilter(c,e,tp)
	return c:IsCanBeSpecialSummoned(e,0,tp,false,false) and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c82308875.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local d1,d2=Duel.TossDice(tp,2)
	if d2>d1 then d1,d2=d2,d1 end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_ATTACK)
	e1:SetValue(d1*700)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,2)
	c:RegisterEffect(e1)
	if d1+d2==7 then
		local b1=Duel.GetFieldGroupCount(tp,LOCATION_ONFIELD,LOCATION_ONFIELD)>1
		local spg=Duel.GetMatchingGroup(c82308875.spfilter,tp,LOCATION_HAND+LOCATION_GRAVE,LOCATION_GRAVE,nil,e,tp)
		local b2=Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and spg:GetCount()>0
		local b3=Duel.IsPlayerCanDraw(tp,3)
		if not b1 and not b2 and not b3 then return end
		local op=0
		if b1 and not b2 and not b3 then op=Duel.SelectOption(tp,aux.Stringid(82308875,1))
		elseif not b1 and b2 and not b3 then op=Duel.SelectOption(tp,aux.Stringid(82308875,2))+1
		elseif not b1 and not b2 and b3 then op=Duel.SelectOption(tp,aux.Stringid(82308875,3))+2
		elseif b1 and b2 and not b3 then op=Duel.SelectOption(tp,aux.Stringid(82308875,1),aux.Stringid(82308875,2))
		elseif b1 and not b2 and b3 then op=Duel.SelectOption(tp,aux.Stringid(82308875,1),aux.Stringid(82308875,3)) if op==1 then op=2 end
		elseif not b1 and b2 and b3 then op=Duel.SelectOption(tp,aux.Stringid(82308875,2),aux.Stringid(82308875,3))+1
		else op=Duel.SelectOption(tp,aux.Stringid(82308875,1),aux.Stringid(82308875,2),aux.Stringid(82308875,3)) end
		if op==0 then
			local g=Duel.GetMatchingGroup(aux.TRUE,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,e:GetHandler())
			Duel.SendtoGrave(g,REASON_EFFECT)
		elseif op==1 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			local sg=spg:Select(tp,1,1,nil)
			Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
		else
			Duel.Draw(tp,3,REASON_EFFECT)
			Duel.BreakEffect()
			Duel.DiscardHand(tp,aux.TRUE,2,2,REASON_EFFECT+REASON_DISCARD)
		end
	end
end
