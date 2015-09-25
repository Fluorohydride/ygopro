--彼岸の悪鬼 バルバリッチャ
function c81992475.initial_effect(c)
	--self destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_SELF_DESTROY)
	e1:SetCondition(c81992475.sdcon)
	c:RegisterEffect(e1)
	--Special Summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(81992475,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_HAND)
	e2:SetCountLimit(1,81992475)
	e2:SetCondition(c81992475.sscon)
	e2:SetTarget(c81992475.sstg)
	e2:SetOperation(c81992475.ssop)
	c:RegisterEffect(e2)
	--remove
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(81992475,1))
	e3:SetCategory(CATEGORY_REMOVE+CATEGORY_DAMAGE)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DELAY)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetCountLimit(1,81992475)
	e3:SetTarget(c81992475.rmtg)
	e3:SetOperation(c81992475.rmop)
	c:RegisterEffect(e3)
end
function c81992475.sdfilter(c)
	return not c:IsFaceup() or not c:IsSetCard(0xb1)
end
function c81992475.sdcon(e)
	return Duel.IsExistingMatchingCard(c81992475.sdfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c81992475.filter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c81992475.sscon(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(c81992475.filter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c81992475.sstg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c81992475.ssop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
function c81992475.rmfilter(c)
	return c:IsSetCard(0xb1) and not c:IsCode(81992475) and c:IsAbleToRemove()
end
function c81992475.rmtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c81992475.rmfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c81992475.rmfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c81992475.rmfilter,tp,LOCATION_GRAVE,0,1,3,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,g:GetCount()*300)
end
function c81992475.rmop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	if g:GetCount()>0 and Duel.Remove(g,POS_FACEUP,REASON_EFFECT)~=0 then
		local rg=Duel.GetOperatedGroup()
		local ct=rg:FilterCount(Card.IsLocation,nil,LOCATION_REMOVED)
		if ct>0 then
			Duel.Damage(1-tp,ct*300,REASON_EFFECT)
		end
	end
end
