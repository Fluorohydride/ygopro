--ライトロード・ウォリアー ガロス
function c59019082.initial_effect(c)
	--discard deck
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCategory(CATEGORY_DECKDES+CATEGORY_DRAW)
	e1:SetDescription(aux.Stringid(59019082,0))
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c59019082.condtion)
	e1:SetTarget(c59019082.target)
	e1:SetOperation(c59019082.operation)
	c:RegisterEffect(e1)
end
function c59019082.cfilter(c)
	return c:IsPreviousLocation(LOCATION_DECK)
end
function c59019082.condtion(e,tp,eg,ep,ev,re,r,rp)
	if not re then return false end
	local rc=re:GetHandler()
	return rp==tp and bit.band(r,REASON_EFFECT)~=0 and not rc:IsCode(59019082) and rc:IsSetCard(0x38) and rc:IsType(TYPE_MONSTER)
		and re:GetActivateLocation()==LOCATION_MZONE and eg:IsExists(c59019082.cfilter,1,nil)
end
function c59019082.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,nil,0,tp,2)
end
function c59019082.filter(c)
	return c:IsLocation(LOCATION_GRAVE) and c:IsSetCard(0x38) and c:IsType(TYPE_MONSTER)
end
function c59019082.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:GetControler()~=tp or not c:IsRelateToEffect(e) or c:IsFacedown() then return end
	Duel.DiscardDeck(tp,2,REASON_EFFECT)
	local dg=Duel.GetOperatedGroup()
	local d=dg:FilterCount(c59019082.filter,nil)
	if d>0 then Duel.Draw(tp,d,REASON_EFFECT) end
end
