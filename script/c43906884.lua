--BF－マイン
function c43906884.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(43906884,0))
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c43906884.con)
	e1:SetTarget(c43906884.tg)
	e1:SetOperation(c43906884.op)
	c:RegisterEffect(e1)
end
function c43906884.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x33)
end
function c43906884.con(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(r,0x41)==0x41 and rp~=tp
		and bit.band(e:GetHandler():GetPreviousLocation(),LOCATION_ONFIELD)~=0
		and bit.band(e:GetHandler():GetPreviousPosition(),POS_FACEDOWN)~=0
		and Duel.IsExistingMatchingCard(c43906884.filter,tp,LOCATION_MZONE,0,1,nil)
end
function c43906884.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,1000)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c43906884.op(e,tp,eg,ep,ev,re,r,rp)
	if not Duel.IsExistingMatchingCard(c43906884.filter,tp,LOCATION_MZONE,0,1,nil) then return end
	Duel.Damage(1-tp,1000,REASON_EFFECT)
	Duel.Draw(tp,1,REASON_EFFECT)
end
