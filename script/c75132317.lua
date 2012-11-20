--雄炎星－スネイリン
function c75132317.initial_effect(c)
	--set
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(75132317,0))
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetProperty(EFFECT_FLAG_DELAY)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCondition(c75132317.setcon)
	e1:SetTarget(c75132317.settg)
	e1:SetOperation(c75132317.setop)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(75132317,1))
	e2:SetCategory(CATEGORY_DRAW)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c75132317.drcon)
	e2:SetCost(c75132317.drcost)
	e2:SetTarget(c75132317.drtg)
	e2:SetOperation(c75132317.drop)
	c:RegisterEffect(e2)
end
function c75132317.tgfilter(c,tp)
	return c:IsControler(tp) and c:IsSetCard(0x7c) and c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c75132317.setcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c75132317.tgfilter,1,nil,tp)
end
function c75132317.filter(c)
	return c:IsSetCard(0x7c) and c:IsType(TYPE_TRAP) and c:IsSSetable()
end
function c75132317.settg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) and e:GetHandler():IsFaceup()
		and not e:GetHandler():IsStatus(STATUS_CHAINING) and Duel.GetLocationCount(tp,LOCATION_SZONE)>0
		and Duel.IsExistingMatchingCard(c75132317.filter,tp,LOCATION_DECK,0,1,nil) end
end
function c75132317.setop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SET)
	local g=Duel.SelectMatchingCard(tp,c75132317.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SSet(tp,g:GetFirst())
		Duel.ConfirmCards(1-tp,g)
	end
end
function c75132317.drcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)==1
end
function c75132317.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x7c) and c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsAbleToGraveAsCost()
end
function c75132317.drcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c75132317.cfilter,tp,LOCATION_ONFIELD,0,2,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c75132317.cfilter,tp,LOCATION_ONFIELD,0,2,2,nil)
	Duel.SendtoGrave(g,REASON_COST)
end
function c75132317.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c75132317.drop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
