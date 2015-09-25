--彼岸の悪鬼 ドラゴネル
function c45593826.initial_effect(c)
	--self destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_SELF_DESTROY)
	e1:SetCondition(c45593826.sdcon)
	c:RegisterEffect(e1)
	--Special Summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(45593826,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_HAND)
	e2:SetCountLimit(1,45593826)
	e2:SetCondition(c45593826.sscon)
	e2:SetTarget(c45593826.sstg)
	e2:SetOperation(c45593826.ssop)
	c:RegisterEffect(e2)
	--to deck top
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(45593826,1))
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetCountLimit(1,45593826)
	e3:SetTarget(c45593826.dttg)
	e3:SetOperation(c45593826.dtop)
	c:RegisterEffect(e3)
end
function c45593826.sdfilter(c)
	return not c:IsFaceup() or not c:IsSetCard(0xb1)
end
function c45593826.sdcon(e)
	return Duel.IsExistingMatchingCard(c45593826.sdfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c45593826.filter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c45593826.sscon(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(c45593826.filter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c45593826.sstg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c45593826.ssop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
function c45593826.dttg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsSetCard,tp,LOCATION_DECK,0,1,nil,0xb1) end
end
function c45593826.dtop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(45593826,2))
	local g=Duel.SelectMatchingCard(tp,Card.IsSetCard,tp,LOCATION_DECK,0,1,1,nil,0xb1)
	local tc=g:GetFirst()
	if tc then
		Duel.ShuffleDeck(tp)
		Duel.MoveSequence(tc,0)
		Duel.ConfirmDecktop(tp,1)
	end
end
