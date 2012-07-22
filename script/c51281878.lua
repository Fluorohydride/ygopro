--フィッシュボーグ－プランター
function c51281878.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(51281878,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_NO_TURN_RESET)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCountLimit(1)
	e1:SetTarget(c51281878.cost)
	e1:SetTarget(c51281878.target)
	e1:SetOperation(c51281878.operation)
	c:RegisterEffect(e1)
end
function c51281878.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,51281878)==0 end
	Duel.RegisterFlagEffect(tp,51281878,RESET_PHASE+PHASE_END,EFFECT_FLAG_OATH,1)
end
function c51281878.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>0
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,nil,0,tp,1)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c51281878.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)==0 then return end
	Duel.DiscardDeck(tp,1,REASON_EFFECT)
	local c=e:GetHandler()
	local tc=Duel.GetOperatedGroup():GetFirst()
	if tc and c:IsRelateToEffect(e) and tc:IsLocation(LOCATION_GRAVE) and tc:IsAttribute(ATTRIBUTE_WATER) then
		Duel.BreakEffect()
		Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)
	end
end
